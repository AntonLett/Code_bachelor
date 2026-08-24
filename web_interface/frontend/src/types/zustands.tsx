import { create } from "zustand";
import {
  selectTypeSchema,
  type dateFieldType,
  type selectType,
  type textType,
} from "./types_zod";

//from Mistral/ChatAi 30.03.2026
type FilterStore = {
  filters: Record<string, string>;
  setFilters: (name: string, value: string) => void;
  resetFilters: () => void;
  filtersActive: boolean;
  setFiltersActive: (active: boolean) => void;
  resetFiltersActive: () => void;
};

const getInitialFilters = (
  components: (selectType | textType | dateFieldType)[],
): Record<string, string> => {
  const initialFilters: Record<string, string> = {};
  components.forEach((comp) => {
    if (selectTypeSchema.safeParse(comp).success) {
      initialFilters[comp.filterName] = comp.currentOption;
    }
  });

  return initialFilters;
};

export const createFilterStore = (
  components: (selectType | textType | dateFieldType)[],
) => {
  const initialFilters = getInitialFilters(components);

  return create<FilterStore>((set) => ({
    filters: initialFilters,
    filtersActive: false,
    setFilters: (name, value) =>
      set((state) => ({
        filters: { ...state.filters, [name]: value },
      })),
    resetFilters: () => set({ filters: initialFilters }),

    setFiltersActive: (active: boolean) => set({ filtersActive: active }),
    resetFiltersActive: () => set({ filtersActive: false }),
  }));
};

// Source - ChatAI - 21.08.2026
type SearchDocument = {
  id: string;
  index: string;
  score: number;
  source: Record<string, unknown>;
};

type SearchState = {
  documents: SearchDocument[];
  totalHits: number;
  error: string | null;
  setResults: ({
    hits,
    total,
  }: {
    hits: SearchDocument[];
    total: number;
  }) => void;
  clearResults: () => void;
};

export const useSearchStore = create<SearchState>((set) => ({
  documents: [],
  totalHits: 0,
  isLoading: false,
  error: null,
  setResults: ({ hits, total }) =>
    set({
      documents: hits,
      totalHits: total,
      error: null,
    }),
  clearResults: () =>
    set({
      documents: [],
      totalHits: 0,
      error: null,
    }),
}));
