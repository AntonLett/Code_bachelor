import { AppBar, Toolbar, Typography, Button, Container, ButtonGroup } from '@mui/material';
import { Children, type ReactElement, type ReactNode } from 'react';

export default function MyNavbar({children}: {children?:ReactNode}) {
  return (
    <AppBar position="static" color='primary'>
      <Toolbar>
        <Typography variant="h6" sx={{ flexGrow: 1 }}>
          MetadataManagement
        </Typography>
      {Children.map(children, child =>
        child
      )}
      </Toolbar>
    </AppBar>
  );
}

export function MyNavbar2({children} : {children?:ReactNode}){

  return (
    <>
      <div>
        <AppBar position='static'>
          <Container maxWidth="sm">
            <div>ABC</div>
            <div>EFG</div>
          </Container>
        </AppBar>
      </div>
    </>
  )
}