import MyBar from './myBarChart';
import Card from '@mui/material/Card'
import MyPie from './myPieChart';
import { Box, Button, CardContent, Typography } from '@mui/material';
import { useEffect, useState } from 'react';
import type { DataItem } from '../types/types'

const API_BASE_URL = import.meta.env.VITE_API_BASE_URL

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

    
export default function MyStartingPage(){
    const [typeCount, setTypeCount] = useState<DataItem[]>([])
    const [loading, setLoading] = useState<Boolean>(true);
    const [groupSizes, setgroupSizes] = useState<DataItem[]>([])

    useEffect(() => {
    const controller = new AbortController();
    const signal = controller.signal;

    const fetchData = async () => {
        try {
            setLoading(true);
            const [groupSizes, filetypeCount] = await Promise.all([
            fetch(API_BASE_URL+`/sum?index=fourth_try&field=FileGroupID&sum=FileSize`,{signal}),
            fetch(API_BASE_URL+'/count?index=fourth_try&field=FileType', {signal})
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
        loading ? <><h1>Loading ...</h1></> :
    <>
        <Box
            sx={{
            display: 'flex',
            flexWrap: 'wrap',
            justifyContent: 'center',
            gap: 2,
            p: 2
            }}
        >
            <Card variant="outlined" sx={{ flex: 'auto', minHeight: 350 }}>
                <CardContent>
                    <MyPie data={typeCount} height={300} />
                </CardContent>
                <CardContent>
                    <Typography variant="h6" component="div" sx={{ textAlign: 'center' }}>
                        Storage usage per group (in Byte)
                    </Typography>
                </CardContent>
            </Card>
            <Card variant="outlined" sx={{ flex: 'auto', minHeight: 350 }}>
            <MyBar data={typeCount} height={300} />
            </Card>
            <Card variant="outlined" sx={{ flex: 'auto', minHeight: 350 }}>
            <MyBar data={typeCount} height={300} />
            </Card>
            <Card variant="outlined" sx={{ flex: 'auto', minHeight: 350 }}>
            <MyBar data={typeCount} height={300} />
            </Card>
        </Box>
        <Box 
            sx={{
                display: 'flex',
                flexWrap: 'wrap',
                justifyContent: 'center',
                gap: 2,
                p: 2
            }}
        >
            <Card variant="outlined" sx={{ flex: 'auto', minHeight: 350 }}>
            <MyBar data={typeCount} height={300} />
            </Card>
            <Card variant="outlined" sx={{ flex: 'auto', minHeight: 350 }}>
            <MyBar data={typeCount} height={300} />
            </Card>
        </Box>
    </>
    )
}