import { CardContent, CardMedia, Typography } from "@mui/material";

function MyCard() {

    return (
        <>
            <CardMedia>
                <MyPie data={data} height={300} />
            </CardMedia>
            <CardContent>
                <Typography variant="h5" component="div">
                    Storage usage per group (in Byte)
                </Typography>
            </CardContent>
        </>
    )
}