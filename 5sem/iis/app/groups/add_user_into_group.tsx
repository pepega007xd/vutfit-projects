"use client"

import { Users } from "@/types";
import React, { useEffect, useState } from "react";
import { Button, Dialog, DialogActions, DialogContent, DialogTitle, List, ListItem, ListItemText } from "@mui/material";
import { add_user_to_group, get_user_which_are_not_members } from "@/actions";

type AddUserIntoGroupProps = {
    groupId: number;
    onAddUserIntoGroup: () => void;
};


export default function AddUserIntoGroup({ groupId, onAddUserIntoGroup }: AddUserIntoGroupProps) {
    const [open, setOpen] = useState(false);
    const [potentialUsersForAdd, setPotentialUsers] = useState<Users[]>([]);
    const [error, setError] = useState<string | undefined>(undefined);

    useEffect(() => {
        const fetchUsers = async () => {
            try {
                const data = await get_user_which_are_not_members(groupId);
                setPotentialUsers(data || []);
            } catch (error) {
                setError('Failed to load potential new members for group' + error);
            }
        };

        fetchUsers();
    }, [groupId]);

    const handleAddUserIntoGroup = async (userId: number) => {
        try {
            // prikaz k pridani do skupiny
            await add_user_to_group(groupId, userId);
            // update seznamu
            const data = await get_user_which_are_not_members(groupId);
            setPotentialUsers(data || []);
            onAddUserIntoGroup();
        } catch (error) {
            setError('Failed to load potential new members for group in handleAddUserIntoGroup' + error);
        }
    }

    const handleClickOpen = async () => {


        try {
            const data = await get_user_which_are_not_members(groupId);
            setPotentialUsers(data || []);
            setOpen(true);
        } catch (error) {
            setError('Failed to load potential new members for group' + error);
        }
    }
    const handleClose = () => setOpen(false);


    return <div>
        {error}
        <Button variant="outlined" onClick={handleClickOpen}>Add user</Button>
        <Dialog open={open}>
            <DialogTitle>Choose user: </DialogTitle>
            <DialogContent>
                <List>
                    {potentialUsersForAdd.map((user) => (
                        <ListItem key={user.user_id}>
                            <ListItemText primary={"User: " + user.username} />
                            <Button onClick={() => handleAddUserIntoGroup(user.user_id)}>Add user into group</Button>
                        </ListItem>

                    ))}
                </List>
            </DialogContent>
            <DialogActions>
                <Button onClick={handleClose}>Close</Button>
            </DialogActions>
        </Dialog>
    </div>
}
