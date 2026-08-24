export interface DataItem{
  label: string,
  value: number,
  color?: string,
}

export interface MyPieProps {
  data: DataItem[],
}

export interface SelectedValues{
  category: string,
  sortBy: string,
  filter: string,
  groupName: string,
}

export interface DropdownOptions{
  category: string[]
  sortBy: string[]
  filter: string[]
}

export interface selectType{ //idea from https://www.reddit.com/r/typescript/comments/1atevmh/how_to_make_an_optional_property_required_if/
  options: string[], //menupoints in dropdown
  values: string[], //the value the dropdown points represent
  currentOption: string, //the initial value selected in the dropdown
  lbl: string, //name/label/id of the component
  type: 'selectType',
}

export interface textType{
  name: string,
  type: 'textType',
}

export const settings = {
    MicroscopeType: {
        "Widefield": ["AxioScan 7", "AxioScan.Z1", "AxioZoom"],
        "Confocal": ["LSM710 Imager", "LSM780 Observer", "LSM880 Examiner", "LSM880 Imager", "LSM880 Observer", "LSM980 NIR", "Marianas Spinning Disk"]
    }
}