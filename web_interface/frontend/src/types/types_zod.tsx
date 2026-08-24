import * as z from "zod";

export const DataItemSchema = z.object({
  label: z.string(),
  value: z.number(),
  color: z.string().optional(),
});

export type DataItem = z.infer<typeof DataItemSchema>;

export const MyPiePropsSchema = z.object({
  data: z.array(DataItemSchema),
});

export type MyPieProps = z.infer<typeof MyPiePropsSchema>;

export const SelectedValuesSchema = z.object({
  category: z.string(),
  sortBy: z.string(),
  filter: z.string(),
  groupName: z.string(),
});
export type SelectedValues = z.infer<typeof SelectedValuesSchema>;

export const DropdownOptionsSchema = z.object({
  category: z.array(z.string()),
  sortBy: z.array(z.string()),
  filter: z.array(z.string()),
});
export type DropdownOptions = z.infer<typeof DropdownOptionsSchema>;

export const selectTypeSchema = z
  .object({
    //idea from https://www.reddit.com/r/typescript/comments/1atevmh/how_to_make_an_optional_property_required_if/
    options: z.array(z.string()), //menupoints in dropdown
    values: z.array(z.string()), //the value the dropdown points represent
    currentOption: z.string(), //the initial value selected in the dropdown
    lbl: z.string(), //name/label/id of the component
    filterName: z.string(), //name for the filter
  })
  .strict();
export type selectType = z.infer<typeof selectTypeSchema>;

export const textTypeSchema = z.object({
  name: z.string(),
  filterName: z.string(),
});
export type textType = z.infer<typeof textTypeSchema>;

export const dateFieldTypeSchema = z
  .object({
    defaultDate: z.date().optional(),
    lbl: z.string(),
    filterName: z.string(),
  })
  .strict();
export type dateFieldType = z.infer<typeof dateFieldTypeSchema>;

export const reqFiltersSchema = z.object({
  microscopeType: z.string().optional(),
  group: z.string().optional(),
  newerThan: z.date().optional(),
  olderThan: z.date().optional(),
});
export type reqFiltersType = z.infer<typeof reqFiltersSchema>;
