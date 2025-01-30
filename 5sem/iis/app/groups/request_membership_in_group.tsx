"use client"

import { useEffect, useState } from "react"
import { Groups } from "@/types";
import { get_avaliable_groups, send_join_group_request } from "@/actions";
import { Button, Dialog, DialogActions, DialogContent, DialogTitle, List, ListItem, ListItemText } from "@mui/material";


export function RequestMembershipInGroup() {
    const [avaliableGroups, setGroups] = useState<Groups[]>([])
    const [open, setOpen] = useState(false);

    useEffect(() => {
        const fetchGroups = async () => {
            const data = await get_avaliable_groups();
            setGroups(data);
        };
        fetchGroups();
    }, []);



    const handleRequest = async (groupId: number) => {
        await send_join_group_request(groupId);
        const data = await get_avaliable_groups();
        setGroups(data);
    };
    return <div>
        <Button variant="outlined" onClick={() => setOpen(true)}>Request membership in group</Button>
        <Dialog open={open}>
            <DialogTitle>Avaliable groups:</DialogTitle>
            <DialogContent>
                <List>
                    {avaliableGroups.map((group) => (
                        <ListItem key={group.group_id}>
                            <ListItemText primary={group.group_name} sx={{ paddingRight: 5 }} />
                            <Button onClick={() => handleRequest(group.group_id)} variant="outlined">Request membership</Button>
                        </ListItem>
                    ))}
                </List>
            </DialogContent>
            <DialogActions>
                <Button onClick={() => setOpen(false)}>Close</Button>
            </DialogActions>
        </Dialog>
    </div>
}
