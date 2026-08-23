import {
  Box,
  Card,
  CardContent,
  List,
  ListItem,
  Typography,
} from "@mui/material";
import { useSearchStore } from "../types/zustands";
import { type DataItem } from "../types/types_zod";
import MyPie from "./myPieChart";

const byteToUnit = (bytes: number) => {
  const units = ["Byte", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"];

  let count = 0;
  while (bytes >= 1000 && count < units.length) {
    bytes /= 1000;
    count++;
  }
  return bytes.toFixed(3).toString() + units[count];
};

export default function VisualizeResults() {
  const SearchState = useSearchStore();

  /* // Source - https://stackoverflow.com/a/66523350
    // Posted by Nguyễn Văn Phong, modified by community. See post 'Timeline' for change history
    // Retrieved 2026-08-21, License - CC BY-SA 4.0 */
  const groupedFilesize: DataItem[] = Object.values(
    SearchState.documents.reduce(
      (total, value) => (
        (total[value.source.Group] =
          total[value.source.Group] ||
          ({
            label: value.source.Group,
            value: 0,
          } as DataItem)),
        // The more elegant way: total[value.] ??= {name: value., count: 0};
        (total[value.source.Group]["value"] += value.source.FileSize),
        total
      ),
      {} as Record<string, DataItem>,
    ),
  );

  console.log(groupedFilesize);

  return (
    <>
      <Box
        sx={{
          display: "flex",
          flexWrap: "wrap",
          justifyContent: "center",
          gap: 2,
          p: 2,
        }}
      >
        <Card variant="outlined" sx={{ flex: "auto" }}>
          <CardContent>
            <Typography
              variant="h6"
              component="div"
              sx={{ textAlign: "center" }}
            >
              List of all files using chosen microscope
            </Typography>
            <List>
              {SearchState.documents.map((value) => (
                <ListItem key={value.id}>{value.source.FilePath}</ListItem>
              ))}
            </List>
          </CardContent>
        </Card>
        <Card variant="outlined" sx={{ flex: "auto" }}>
          <CardContent>
            <Typography
              variant="h6"
              component="div"
              sx={{ textAlign: "center" }}
            >
              Total size of files containing chosen microscope
            </Typography>
            <h8>
              {byteToUnit(
                SearchState.documents.reduce(
                  (total, value) => (total = total + value.source.FileSize),
                  0,
                ),
              )}
            </h8>
          </CardContent>
        </Card>
        <Card variant="outlined" sx={{ flex: "auto" }}>
          <CardContent>
            <Typography
              variant="h6"
              component="div"
              sx={{ textAlign: "center" }}
            >
              Share of file size per group
            </Typography>

            <MyPie data={groupedFilesize}></MyPie>
          </CardContent>
        </Card>
      </Box>
    </>
  );
}
