"use client"

import { Users } from "@/types";
import React, { useEffect, useState } from "react";
import { Button, Dialog, DialogActions, DialogContent, DialogTitle, List, ListItem, ListItemText } from "@mui/material";
import { add_user_to_group, get_join_group_requests, remove_user_from_request } from "@/actions";

type HandleRequestForGroupProps = {
    groupId: number;
    onAddUserIntoGroup: () => void;
};


export default function HandleRequestForGroup({ groupId, onAddUserIntoGroup }: HandleRequestForGroupProps) {
    const [open, setOpen] = useState(false);
    const [potentialUsersForAdd, setPotentialUsers] = useState<Users[]>([]);
    const [error, setError] = useState<string | undefined>(undefined);

    useEffect(() => {
        const fetchUsers = async () => {
            try {
                const data = await get_join_group_requests(groupId);
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
            await remove_user_from_request(userId, groupId);
            // update seznamu
            const data = await get_join_group_requests(groupId);
            setPotentialUsers(data || []);
            onAddUserIntoGroup();
        } catch (error) {
            setError('Failed to load potential new members for group in handleAddUserIntoGroup' + error);
        }
    }

    const handleDeny = async (userId: number) => {
        try {
            await remove_user_from_request(userId, groupId);
            // update seznamu
            const data = await get_join_group_requests(groupId);
            setPotentialUsers(data || []);
        } catch (error) {
            setError('Failed to remove user from list' + error);
        }
    }

    const handleClickOpen = async () => {


        try {
            const data = await get_join_group_requests(groupId);
            setPotentialUsers(data || []);
            setOpen(true);
        } catch (error) {
            setError('Failed to load potential new members for group' + error);
        }
    }
    const handleClose = () => setOpen(false);


    return <div>
        {error}
        <Button variant="outlined" onClick={handleClickOpen}>Manage requests</Button>
        <Dialog open={open}>
            <DialogTitle>Choose user: </DialogTitle>
            <DialogContent>
                <List>
                    {potentialUsersForAdd.map((user) => (
                        <ListItem key={user.user_id}>
                            <ListItemText primary={"User: " + user.username} />
                            <Button onClick={() => handleAddUserIntoGroup(user.user_id)}>Accept</Button>
                            <Button onClick={() => handleDeny(user.user_id)}>Deny</Button>
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
