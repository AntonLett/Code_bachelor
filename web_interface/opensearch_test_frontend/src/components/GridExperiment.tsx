import { useState, useEffect } from 'react'
import MyPie from './myPieChart';
import type { DataItem } from '../types/types'
import * as React from 'react';
import { styled } from '@mui/material/styles';
import Box from '@mui/material/Box';
import Paper from '@mui/material/Paper';
import {Grid} from '@mui/material';
import Stack from '@mui/material/Stack';
import MyNavbar from './myNavbar';

const jsonToDataItemArray = (jsonData) => {
  const firstItem = jsonData[0];
  const keys = Object.keys(firstItem);
  const labelKey = keys[0];
  const valueKey = keys[1];

  const transformedData: DataItem[] = jsonData.map(item => ({
    label: item[labelKey],
    value: item[valueKey],
  }))

  return transformedData
}

const Item = styled(Paper)(({ theme }) => ({
  backgroundColor: '#fff',
  ...theme.typography.body2,
  padding: theme.spacing(1),
  textAlign: 'center',
  color: theme.palette.text.secondary,
  ...theme.applyStyles('dark', {
    backgroundColor: '#1A2027',
  }),
}));

const byteToUnit = (bytes: number) => {
  const units = ["Byte","KB","MB","GB","TB","PB","EB","ZB","YB"];

  let count = 0
  while(bytes >= 1000 && count < units.length ) {
    bytes /= 1000;
    count++;
  }
  return bytes.toFixed(3).toString() + units[count]
}

function App() {
  const [groupSizes, setgroupSizes] = useState<DataItem[]>([])
  const [typeCount, setTypeCount] = useState<DataItem[]>([])
  const [loading, setLoading] = useState<Boolean>(false);

  console.log(byteToUnit(123))

  useEffect(() => {
    const controller = new AbortController();
    const signal = controller.signal;

    const fetchData = async () => {
      try {
        setLoading(true);
        const [groupSizes, filetypeCount] = await Promise.all([
          fetch(`http://localhost:3000/api/sum?index=fourth_try&field=FileGroupID&sum=FileSize`,{signal}),
          fetch('http://localhost:3000/api/count?index=fourth_try&field=FileType', {signal})
        ]);

        if(!groupSizes.ok) {
          throw new Error(`HTTP error! Status: ${groupSizes.status}`)
        }

        const jsonData = await groupSizes.json();
        setgroupSizes(jsonToDataItemArray(jsonData));

        const typeData = await filetypeCount.json();
        setTypeCount(jsonToDataItemArray(typeData))
      } catch(error) {
        if (error.name !== 'AbortError') {
          console.error("Error while loading data: ", error)
        }
      } finally {
        setLoading(false);
      }
    };

    fetchData();

    return () => {
      controller.abort();
    };
  }, []);

return (
  <>
    <MyNavbar />
    <Grid container spacing={2}>
      <Grid size={4}>
        <Stack spacing={2}>
          <Item>Column 1 - Row 1</Item>
          <Item>Column 1 - Row 2</Item>
          <Item>Column 1 - Row 3</Item>
        </Stack>
      </Grid>
      <Grid size={8}>
        <Item sx={{ height: '100%', boxSizing: 'border-box' }}>Column 2</Item>
      </Grid>
    </Grid>
  </>
);
}

export default App
