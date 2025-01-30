"use client"
import { DialogActions, DialogContent, IconButton } from "@mui/material";
import React, { useEffect } from 'react';
import Button from '@mui/material/Button';
import List from '@mui/material/List';
import ListItem from '@mui/material/ListItem';
import ListItemButton from '@mui/material/ListItemButton';
import ListItemText from '@mui/material/ListItemText';
import DialogTitle from '@mui/material/DialogTitle';
import Dialog from '@mui/material/Dialog';
import { ShareSharp } from "@mui/icons-material";
import { get_users_for_share, share_post } from "@/actions";
import { Users } from "@/types";


type SharepostProps = {
  id_photo: number;
}

export default function Sharepost({ id_photo }: SharepostProps) {
  const [open, setOpen] = React.useState(false);

  const [users, setSelectedValue] = React.useState<Users[]>([]);

  const fetchselectedValues = async () => {
    const idk = await get_users_for_share(id_photo);
    setSelectedValue(idk);
  }

  useEffect(() => {
    const fetchselectedValues = async () => {
      const idk = await get_users_for_share(id_photo);
      setSelectedValue(idk);
    };
    fetchselectedValues();
  }, [id_photo]);

  const handleClickOpen = () => {
    setOpen(true);
  };

  const handleClose = () => {
    setOpen(false);
  };

  const handleshare = async (photo_id: number, reciever_id: number) => {
    await share_post(photo_id, reciever_id);
    fetchselectedValues();
  };

  return (
    <div>
      <IconButton onClick={handleClickOpen}>
        <ShareSharp />
      </IconButton>
      <Dialog open={open} onClose={() => setOpen(false)}>
        <DialogTitle>Share to:</DialogTitle>
        <DialogContent>
          <List>
            {
              users.map(
                (user_member) => (
                  <ListItem key={user_member.user_id}>
                    <ListItemButton onClick={() => handleshare(id_photo, user_member.user_id)}
                    >
                      <ListItemText primary={user_member.username} />
                    </ListItemButton>
                  </ListItem>
                )
              )
            }
          </List>
        </DialogContent>
        <DialogActions >
          <Button onClick={handleClose} >Close</Button>
        </DialogActions>

      </Dialog>
    </div>
  );
}
