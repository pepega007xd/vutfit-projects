"use client"

import { useEffect, useState } from "react";
import { Users } from "@/types";
import { List, ListItem } from "@mui/material";
import { get_banned_users } from "@/actions";
import UserDetails from "@/user/[user_id]/profile";


export default function UserDetailList() {
    const [users, setUsers] = useState<Users[]>([]);
    const [error, setError] = useState<string | undefined>(undefined);
    useEffect(() => {
        const fetchUsers = async () => {
            try {
                const data = await get_banned_users();
                setUsers(data);
            } catch (error) {
                setError("Failed to load users" + error);
            }
        }
        fetchUsers();
    }, []);
    return (
        <div>
            {error}
            <List>
                {users.map((user) => (
                    <ListItem key={user.user_id}>
                        <UserDetails userId={user.user_id} />
                    </ListItem>
                ))}
            </List>
        </div>
    );
}
