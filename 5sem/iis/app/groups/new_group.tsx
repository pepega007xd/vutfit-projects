"use client"

import React, { useState } from 'react';
import { Button, Dialog, DialogActions, DialogContent, DialogTitle, TextField } from '@mui/material';
import { create_group } from "@/actions";

type NewGroupProps = {
  onGroupAdded: () => void; // Callback pro přidání nové skupiny
};

export default function New_Group({ onGroupAdded }: NewGroupProps) {
  const [open, setOpen] = useState(false);
  const [groupName, setGroupName] = useState('');
  const [groupDescription, setGroupDescription] = useState('');

  const [error, setError] = useState<string | undefined>(undefined);

  const handleClickOpen = () => setOpen(true);
  const handleClose = () => setOpen(false);

  const handleSubmit = async () => {
    setError(undefined);

    try {
      const newGroup = await create_group(groupName, groupDescription);
      if (newGroup) {
        onGroupAdded();
      }
      handleClose();
    } catch (error) {
      setError('Failed to create group' + error);
    }


  };

  return (
    <div>
      {error}
      <Button variant="outlined" onClick={handleClickOpen}>
        Create group
      </Button>
      <Dialog open={open} onClose={handleClose}>
        <DialogTitle>Enter Group Details</DialogTitle>
        <DialogContent>
          <TextField
            autoFocus
            margin="dense"
            label="Group Name (required)"
            fullWidth
            value={groupName}
            onChange={(e) => setGroupName(e.target.value)}
          />
          <TextField
            margin="dense"
            label="Description"
            fullWidth
            value={groupDescription}
            onChange={(e) => setGroupDescription(e.target.value)}
          />
        </DialogContent>
        <DialogActions>
          <Button onClick={handleClose} color="primary">
            Cancel
          </Button>
          <Button onClick={handleSubmit} color="primary" disabled={groupName == '' ? true : false}>
            Create
          </Button>
        </DialogActions>
      </Dialog>
    </div >
  );
}
