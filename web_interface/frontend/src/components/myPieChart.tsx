import { type DefaultizedPieValueType } from '@mui/x-charts/models'; 
import { PieChart, pieArcLabelClasses } from '@mui/x-charts/PieChart';
import type { DataItem, MyPieProps } from '../types/types'

const sizing = {
  margin: { right: 5 },
  width: 200,
  height: 200,
  hideLegend: false,
};


const getArcLabel = (params: DefaultizedPieValueType, data: DataItem[]) => {
  const TOTAL = data.map((item) => item.value).reduce((a, b) => a + b, 0);
  const percent = params.value / TOTAL;
  return `${(percent * 100).toFixed(2)}%`;
};



function MyPie ({data}: MyPieProps) {
    return (
        <>
          <PieChart onItemClick={(event,item,params) => console.log(params.label)}
            series={[
            {
                outerRadius: 80,
                data,
                // arcLabel: getArcLabel,
                highlightScope: {fade: 'global', highlight: 'item'},
                faded: { additionalRadius: -15, color: 'gray' },
            },   
          ]}
          sx={{
            [`& .${pieArcLabelClasses.root}`]: {
                fill: 'white',
                fontSize: 14,
            },
          }}
          {...sizing}
          />
        </>
    )
}

export default MyPie