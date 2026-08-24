import {
  Box,
  Button,
  Select,
  MenuItem,
  FormControl,
  InputLabel,
  TextField,
  type SelectChangeEvent,
} from "@mui/material";
import { AdapterDayjs } from "@mui/x-date-pickers/AdapterDayjs";
import dayjs from "dayjs";
import "dayjs/locale/de";
import { DateField, LocalizationProvider } from "@mui/x-date-pickers";
import {
  type selectType,
  type textType,
  type dateFieldType,
  selectTypeSchema,
  textTypeSchema,
  dateFieldTypeSchema,
  // type reqFiltersType,
} from "../types/types_zod";
import { useFilterStore } from "../types/constants";
// import { opensearchQueries } from "../types/requests";
import axios from "axios";
import { useSearchStore } from "../types/zustands";
interface infoForSelect {
  components: (selectType | textType | dateFieldType)[];
}

export default function MyFilterBar({ components }: infoForSelect) {
  const {
    filters,
    setFilters,
    resetFilters,
    filtersActive,
    setFiltersActive,
    resetFiltersActive,
  } = useFilterStore();

  const { setResults, clearResults } = useSearchStore();
  const sendAPICall = async () => {
    try {
      console.log("Button pressed! ", filters);

      setFiltersActive(true);
      console.log(filters.newerThan);
      //TODO: CARE FOR TIMEZONE!!!!!!!!!!! TIMEZONE gets converted to utc when transmitting, so 01/01/2025 00:00:00 GMT+0100 becomes 31/12/2024 23:00:00 after transmission
      const response = await axios.get("http://localhost:3000/api/filters", {
        params: filters,
      });
      console.log("Filters: ", filters);
      // Source - ChatAI - 21.08.2026
      const formattedHits = response.data.body.hits.hits.map((doc: any) => ({
        id: doc._id,
        index: doc._index,
        score: doc._score,
        source: doc._source,
      }));
      setResults({
        hits: formattedHits,
        total: response.data.body.hits.total.value,
      });
    } catch (error) {
      console.error(error);
    }
    //TODO: WELCHE FILTER GIBT ES? WIE API CALL AUFBAUEN, WENN FILTER NICHT VON VORNE REIN BEKANNT?
  };

  const resetFiltersList = () => {
    resetFiltersActive();
    resetFilters();
    clearResults();
  };

  const handleChange = (
    //ChatAI | Mistral Large 3 675B Instruct 2512 | OpenSearch Query Logic | 02.April 2026
    eventOrName:
      | React.ChangeEvent<HTMLInputElement>
      | SelectChangeEvent<string>
      | string,
    value?: unknown,
  ) => {
    let name: string;
    let val: unknown;

    // Fall 1: TextField oder Select (event.target.name & event.target.value)
    if (typeof eventOrName === "object" && "target" in eventOrName) {
      name = eventOrName.target.name;
      val = eventOrName.target.value;
    }
    // Fall 2: DatePicker (name als String, value direkt)
    else {
      name = eventOrName as string;
      val = value.$d ? dayjs(value).toDate() : undefined;
    }

    setFilters(name, val);
  };

  return (
    <Box
      sx={{
        width: "100vm",
        padding: 1,
        backgroundColor: "#f5f5f5",
        borderRadius: 1,
      }}
    >
      <Box
        sx={{
          display: "flex",
          gap: 2,
          width: "100vm",
        }}
      >
        {components.map((comp) => (
          <FormControl key={"formControl" + comp.filterName} fullWidth>
            {selectTypeSchema.safeParse(comp).success && (
              <>
                <InputLabel id={comp.lbl}>{comp.lbl}</InputLabel>
                <Select
                  labelId={comp.lbl}
                  id={comp.lbl}
                  name={comp.filterName}
                  value={filters[comp.filterName]}
                  label={comp.lbl}
                  key={comp.lbl}
                  onChange={handleChange}
                >
                  {comp.options.map((option, index) => (
                    <MenuItem
                      key={comp.lbl + option}
                      value={comp.values[index]}
                    >
                      {option}
                    </MenuItem>
                  ))}
                </Select>
              </>
            )}
            {textTypeSchema.safeParse(comp).success && (
              <>
                <TextField
                  id={comp.name}
                  name={comp.filterName}
                  label={comp.name}
                  key={comp.name}
                  variant="outlined"
                  onChange={handleChange}
                />
              </>
            )}
            {/*https://mui.com/x/react-date-pickers/adapters-locale/ 02.April 2026*/}
            {dateFieldTypeSchema.safeParse(comp).success && (
              <>
                <LocalizationProvider
                  dateAdapter={AdapterDayjs}
                  adapterLocale="de"
                >
                  <DateField
                    format="DD/MM/YYYY"
                    label={comp.lbl}
                    {...(comp.defaultDate && {
                      defaultValue: dayjs(comp.defaultDate),
                    })}
                    onChange={(value) => handleChange(comp.filterName, value)}
                  />
                </LocalizationProvider>
              </>
            )}
          </FormControl>
        ))}
        <FormControl fullWidth>
          <Button onClick={sendAPICall}>Apply Filters</Button>
          <Button onClick={resetFiltersList}>
            {filtersActive ? "Return to Start" : "Reset Filters"}
          </Button>
        </FormControl>
      </Box>
    </Box>
  );
}
