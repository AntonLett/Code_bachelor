import React, { useState } from "react";
import {
  Box,
  Select,
  MenuItem,
  FormControl,
  InputLabel,
  TextField,
  type SelectChangeEvent
} from "@mui/material";
import { type SelectedValues, type DropdownOptions } from "../types/types";


//TODO: STATE AUSLAGERN
interface Props{
    selectedValues: SelectedValues,
    dropdownOptions: DropdownOptions,
    handleChange: (event: SelectChangeEvent<string>, child: React.ReactNode) => void,
}

export default function FullWidthDropdownBar({selectedValues, dropdownOptions, handleChange}: Props) {
  // Beispiel-Daten für die Dropdowns
  

  return (
    <Box
      sx={{
        width: "100vm", // Volle Breite des Containers
        padding: 1,
        backgroundColor: "#f5f5f5", // Optional: Hintergrundfarbe
        borderRadius: 1, // Optional: Abgerundete Ecken
      }}
    >
      <Box
        sx={{
          display: "flex",
          gap: 2,
          width: "100vm",
        }}
      >
        <FormControl fullWidth>
          <InputLabel id="category-label">Kategorie</InputLabel>
          <Select
            labelId="category-label"
            id="category"
            name="category"
            value={selectedValues.category}
            label="Kategorie"
            onChange={handleChange}
          >
            {dropdownOptions.category.map((option) => (
              <MenuItem key={option} value={option}>
                {option}
              </MenuItem>
            ))}
          </Select>
        </FormControl>

        {/* Dropdown 2: Sortierung */}
        <FormControl fullWidth>
          <InputLabel id="sortBy-label">Sortieren nach</InputLabel>
          <Select
            labelId="sortBy-label"
            id="sortBy"
            name="sortBy"
            value={selectedValues.sortBy}
            label="Sortieren nach"
            onChange={handleChange}
          >
            {dropdownOptions.sortBy.map((option) => (
              <MenuItem key={option} value={option}>
                {option}
              </MenuItem>
            ))}
          </Select>
        </FormControl>

        {/* Dropdown 3: Filter */}
        <FormControl fullWidth>
          <InputLabel id="filter-label">Filter</InputLabel>
          <Select
            labelId="filter-label"
            id="filter"
            name="filter"
            value={selectedValues.filter}
            label="Filter"
            onChange={handleChange}
          >
            {dropdownOptions.filter.map((option) => (
              <MenuItem key={option} value={option}>
                {option}
              </MenuItem>
            ))}
          </Select>
        </FormControl>
        <FormControl fullWidth>
            <TextField id="groupName" name="groupName" label="Group Name" variant="outlined" onChange={handleChange}/>
        </FormControl>
      </Box>
    </Box>
  );
}