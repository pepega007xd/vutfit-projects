// File: app/layout.tsx
// Author: Tomáš Brablec (xbrabl04)
// Date: 2024-12-13

import Drawer from '@mui/material/Drawer';
import Toolbar from '@mui/material/Toolbar';
import List from '@mui/material/List';
import Divider from '@mui/material/Divider';
import ListItem from '@mui/material/ListItem';
import ListItemButton from '@mui/material/ListItemButton';
import ListItemIcon from '@mui/material/ListItemIcon';
import ListItemText from '@mui/material/ListItemText';
import { Home, AddBox, Folder, FolderShared } from '@mui/icons-material';
import { Typography } from '@mui/material';

const drawerWidth = 230;

// Navigation bar on the left side of the screen
export default function Sidebar() {
  // links to different pages in the application
  const links: [string, string, React.JSX.Element][] = [
    ['Home', "/", <Home key="home" />],
    ['New Post', "/new_post", <AddBox key="newpost" />],
    ['Public Galleries', "/public", <FolderShared key="public_galleries" />],
    ['My Galleries', "/galleries", <Folder key="galleries" />],
  ];

  return (
    <Drawer
      sx={{
        width: drawerWidth,
        flexShrink: 0,
        '& .MuiDrawer-paper': {
          width: drawerWidth,
          boxSizing: 'border-box',
        },
      }}
      variant="permanent"
      anchor="left"
    >
      {/* application name */}
      <Toolbar>
        <Typography variant='h4'>
          Artiscope
        </Typography>
      </Toolbar>

      <Divider />

      {/* list of links to pages */}
      <List>
        {links.map(([name, link, icon]) => (
          <ListItem key={name} disablePadding>
            <ListItemButton href={link}>
              <ListItemIcon>
                {icon}
              </ListItemIcon>
              <ListItemText primary={name} />
            </ListItemButton>
          </ListItem>
        ))}
      </List>
    </Drawer>
  );
}
