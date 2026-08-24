import React from 'react';
import { BarChart } from '@mui/x-charts/BarChart';

// Interface für deine Daten
export interface DataItem {
  label: string;
  value: number;
  color?: string;
}

interface BarChartProps {
  data: DataItem[];
}

export default function MyBar({ data }: BarChartProps) {
  // Extrahiere Labels und Werte für das Diagramm
  const xLabels = data.map(item => item.label);
  const seriesData = data.map(item => item.value);

  return (
    <>
      <BarChart
        xAxis={[
          {
            scaleType: 'band', // Für kategorische Daten (Labels)
            data: xLabels,
            label: 'Amount of files per file type', // X-Achsen-Beschriftung
          },
        ]}
        series={[
          {
            data: seriesData,
            label: 'count', // Legenden-Beschriftung
          },
        ]}
      />
    </>
  );
}