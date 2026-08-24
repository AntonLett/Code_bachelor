import React from "react";
import { Box, Paper, styled } from "@mui/material";

// Container, der die volle Breite einnimmt
const GridContainer = styled(Box)({
  display: "grid",
  gridTemplateColumns: "repeat(auto-fit, minmax(200px, 1fr))", // Automatische Spalten
  gridAutoRows: "100px", // Einheitliche Zeilenhöhe
  gap: "16px",
  width: "100%", // Volle Viewport-Breite
  padding: "16px",
  boxSizing: "border-box", // Padding wird nicht zur Breite gezählt
});

// Kachel mit dynamischer Größe (width/height in "Grid-Einheiten")
const GridTile = styled(Paper)<{ width?: number; height?: number }>(
  ({ width = 1, height = 1 }) => ({
    gridColumn: `span ${width}`, // Breite (Anzahl Spalten)
    gridRow: `span ${height}`,   // Höhe (Anzahl Zeilen)
    display: "flex",
    alignItems: "center",
    justifyContent: "center",
    padding: "16px",
  })
);

const App = () => {
  return (
    <GridContainer>
      <GridTile>1x1 Kachel</GridTile>
      <GridTile width={2}>2x1 Kachel (doppelt so breit)</GridTile>
      <GridTile height={2}>1x2 Kachel (doppelt so hoch)</GridTile>
      <GridTile width={2} height={2}>
        2x2 Kachel (doppelt so breit & hoch)
      </GridTile>
      <GridTile width={3}>3x1 Kachel (dreifach breit)</GridTile>
    </GridContainer>
  );
};

export default App;