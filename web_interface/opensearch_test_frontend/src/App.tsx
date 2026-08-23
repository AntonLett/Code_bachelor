import { useState, useEffect } from "react";
import Card from "@mui/material/Card";
import { Box, Button, List, ListItem } from "@mui/material";
// import { opensearchQueries } from "./types/requests";
import { filterConfig, useFilterStore } from "./types/constants";
import MyFilterBar from "./components/myFilterBar";
import MyStartingPage from "./components/startingPage";
import { useSearchStore } from "./types/zustands";
import VisualizeResults from "./components/visualizeResults";

const API_BASE_URL = import.meta.env.VITE_API_BASE_URL;

function App() {
  const [crawlerBusy, setCrawlerBusy] = useState<boolean>(false);
  const filters = useFilterStore((state) => state.filters);
  const { filtersActive } = useFilterStore();
  const SearchState = useSearchStore();

  function crawlerSpawner(e: React.MouseEvent<HTMLButtonElement>) {
    console.log("Geklickter button: ", e.currentTarget);
    console.log("Button-Text: ", e.currentTarget.textContent);
    console.log("Button ID: ", e.currentTarget.id);
    alert("Button pressed.");
    try {
      fetch(API_BASE_URL + "/spawn");
      // set crawlerBusy to busy
      setCrawlerBusy(true);
    } catch (error) {
      console.log(error);
    }
  }

  useEffect(() => {
    if (crawlerBusy === true) {
      setTimeout(() => {
        setCrawlerBusy(false);
      }, 5000);
    }
  }, [crawlerBusy]);

  useEffect(() => {
    console.log("Change at currentOption: ", filters);
  }, [filters]);

  return (
    <>
      <Box sx={{ padding: "10px" }}>
        {/* <FullWidthDropdownBar selectedValues={selectedValues} dropdownOptions={dropdownOptions} handleChange={handleChange}></FullWidthDropdownBar> */}
        <MyFilterBar components={filterConfig}></MyFilterBar>
        {/* <Card variant="outlined" sx={{ alignSelf: "flex-start" }}>
          <Box sx={{ display: "inline-flex", flexDirection: "column", gap: 1 }}>
            <Button
              variant="contained"
              id="B3"
              disabled={crawlerBusy}
              onClick={crawlerSpawner}
            >
              Spawn Crawler 3
            </Button>
          </Box>
        </Card> */}
        {SearchState.totalHits > 0 ? (
          <div>
            <VisualizeResults></VisualizeResults>
          </div>
        ) : (
          // <MyStartingPage />
          <></>
        )}
      </Box>
    </>
  );
}

export default App;
