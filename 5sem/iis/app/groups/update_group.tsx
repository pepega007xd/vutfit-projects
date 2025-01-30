"use client"

import { update_group } from "@/actions";
import { Groups } from "@/types";
import React, { useState } from "react";
import { Button, Dialog, DialogTitle, DialogContent, DialogActions, TextField } from "@mui/material";

type UpdateGroupProps = {
  group: Groups | undefined;
  onGroupUpdate: () => void;
};

export default function UpdateGroup({ group, onGroupUpdate }: UpdateGroupProps) {
  const [open, setOpen] = useState(false);
  const [groupName, setGroupName] = useState(group?.group_name);
  const [groupDescription, setGroupDescription] = useState(group?.description);

  const [error, setError] = useState<string | undefined>(undefined);

  const handleClickOpen = () => setOpen(true);
  const handleClose = () => setOpen(false);

  const handleSubmit = async () => {
    setError(undefined);

    try {
      if (group && groupName && groupDescription) {
        const updatedGroup = await update_group(group.group_id, groupName, groupDescription);
        if (updatedGroup) {
          onGroupUpdate();
        }
        handleClose();
      }
    } catch (error) {
      setError('Failed to update group' + error);
    }


  };



  return <div>
    {error}
    <Button variant="outlined" color="primary" onClick={handleClickOpen}>Edit group</Button>
    <Dialog open={open} onClose={handleClose}>
      <DialogTitle>Update details of group: {group?.group_name}</DialogTitle>
      <DialogContent>
        <TextField
          autoFocus
          margin="dense"
          label="Set new name"
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
          Update
        </Button>
      </DialogActions>
    </Dialog>
  </div>
}
