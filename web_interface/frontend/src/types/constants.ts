import { createFilterStore } from "./zustands";
import {
  type dateFieldType,
  type selectType,
  type textType,
} from "./types_zod";

const select: selectType = {
  options: ["All", "Widefield", "Confocal", "TXT"],
  values: ["", "widefield", "confocal", "TXT"],
  currentOption: "",
  lbl: "Microscope Type",
  filterName: "microscopeType",
};

const newerThan: dateFieldType = {
  lbl: "Newer Than",
  filterName: "newerThan",
};

const olderThan: dateFieldType = {
  lbl: "Older Than",
  filterName: "olderThan",
};

const text: textType = {
  name: "Group",
  filterName: "group",
};

export const filterConfig = [select, text, newerThan, olderThan];
export const useFilterStore = createFilterStore(filterConfig);

