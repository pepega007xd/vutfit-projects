import Drawer from '@mui/material/Drawer';
import Toolbar from '@mui/material/Toolbar';
import List from '@mui/material/List';
import Divider from '@mui/material/Divider';
import ListItem from '@mui/material/ListItem';
import ListItemButton from '@mui/material/ListItemButton';
import ListItemIcon from '@mui/material/ListItemIcon';
import ListItemText from '@mui/material/ListItemText';
import { Inbox, Home, Login, AddBox, People, AccountCircle, NoAccounts, AdminPanelSettings } from '@mui/icons-material';
import { Button, Typography } from '@mui/material';
import { has_access } from './auth';
import { UserType } from './types';
import { get_current_user } from './actions';
import { cookies } from 'next/headers';
import { redirect } from 'next/navigation';
import { revalidatePath } from 'next/cache';

const drawer_width = 260;

export default async function Sidebar() {
  const user = await get_current_user();

  const links: [string, string, React.JSX.Element, UserType][] = [
    ['Home', "/", <Home key="home" />, "guest"],
    ['New Post', "/new_post", <AddBox key="newpost" />, "user"],
    ['Inbox', "/inbox", <Inbox key="inbox" />, "user"],
    ['Groups', "/groups", <People key="groups" />, "user"],
    ['User management', "/users", <AdminPanelSettings key="users" />, "administrator"],
    ['Banned users', "/userslist", <NoAccounts key="groups" />, "moderator"],
  ];

  return (
    <Drawer
      sx={{
        width: drawer_width,
        flexShrink: 0,
        '& .MuiDrawer-paper': {
          width: drawer_width,
          boxSizing: 'border-box',
        },
      }}
      variant="permanent"
      anchor="left"
    >
      <Toolbar>
        <Typography variant='h4'>
          Fitstagram
        </Typography>
      </Toolbar>
      <List>
        <ListItem disablePadding key="user_button">
          <ListItemButton href={user ? `/user/${user.user_id}` : "/login"}>
            <ListItemIcon>
              {user ? <AccountCircle /> : <Login />}
            </ListItemIcon>
            <ListItemText primary={user ? user.username : "Login"} />
          </ListItemButton>
          {user &&
            <ListItemText>
              <Button onClick={async () => {
                "use server";
                (await cookies()).delete("session");
                revalidatePath("/");
                redirect("/");
              }}>
                Logout
              </Button>
            </ListItemText>
          }
        </ListItem>
        <Divider />
        {links.map(([name, link, icon, permissions]) => (
          <ListItem key={name} disablePadding sx={{
            display: has_access(user?.user_type ?? "guest", permissions) ? "inherit" : "none"
          }}>
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
