"use client"

import React, { useState, useEffect } from "react";
import { get_current_user, get_user, has_moderating_rights, update_biography, update_block_user, update_password, update_username } from "@/actions";
import { Users } from "@/types";
import { Alert, Box, Button, Dialog, DialogActions, DialogContent, DialogTitle, List, ListItem, ListItemText, TextField } from "@mui/material";

type UserDetailsProps = {
    userId: number;
};

export default function UserDetails({ userId }: UserDetailsProps) {
    const [user, setUser] = useState<Users | undefined>(undefined);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState<string | undefined>(undefined);
    useEffect(() => {
        const fetchUsers = async () => {
            try {
                const data = await get_user(userId);
                setUser(data);
            } catch (err) {
                setError("Failed to load user" + err);
            } finally {
                setLoading(false);
            }
        }
        fetchUsers();
    }, [userId]);

    const handleBanUser = async () => {
        try {
            const data = await update_block_user(userId, !user?.is_blocked);
            setUser(data);
        } catch (error) {
            setError("Failed to BAN user" + error);
        }
    };


    // if profile owner -> true
    const [isProfileOwner, setIsProfileOwner] = useState(false);
    useEffect(() => {
        const fetchOwner = async () => {
            const data = await get_current_user();
            if (data?.user_id === userId) setIsProfileOwner(true);
        }
        fetchOwner();
    }, [userId]);

    // state for editing username
    const [userNameDialogOpen, setUserNameDialog] = useState(false);
    const [newUsername, setNewUsername] = useState<string>("");

    const handleUsernameDialogOnClose = () => setUserNameDialog(false);
    const handleUsernameDialogUpdateName = async () => {
        try {
            const data = await update_username(userId, newUsername);
            setUser(data);
            handleUsernameDialogOnClose();
        } catch (error) {
            setError("Failed to update username" + error);
        }
    };

    // state for editing biography
    const [biographyDialogOpen, setBiographyDialog] = useState(false);
    const [newBiography, setNewBiography] = useState<string>("");

    const handleBiographyDialogOnClose = () => setBiographyDialog(false);
    const handleBiogrhaphyDialogUpdateName = async () => {
        try {
            const data = await update_biography(userId, newBiography);
            setUser(data);
            handleBiographyDialogOnClose();
        } catch (error) {
            setError("Failed to update biography" + error);
        }
    };

    // state for editing password
    const [passwordDialogOpen, setPasswordDialog] = useState(false);
    const [newPassword, setNewPassword] = useState("");

    const handlePasswordDialogOnClose = () => setPasswordDialog(false);
    const handlePasswordDialogUpdate = async () => {
        try {
            const data = await update_password(userId, newPassword);
            setUser(data);
            handlePasswordDialogOnClose();
        } catch (error) {
            setError("Failed to update biography" + error);
        }
    };

    const [isModerator, setIsModerator] = useState(false);
    useEffect(() => {
        const fetchModerator = async () => {
            try {
                const data = await has_moderating_rights();
                setIsModerator(data);
            } catch (error) {
                setError("Failed to load moderating status" + error);
            }
        }
        fetchModerator()
    }, []);





    if (loading) return (<Alert severity="info">Loading data...</Alert>);



    return (
        <div>
            {error}
            {user?.is_blocked && <Alert severity="error">User is blocked!</Alert>}
            <Box sx={{
                backgroundColor: '#2B2B2B',
                padding: 2,
                borderRadius: 1,
                boxShadow: 3,
                width: "80vw",
                mx: "auto",
                margin: "2em"
            }}>
                <List>
                    <ListItem>
                        <ListItemText primary={user?.username} secondary="username" />
                        {isProfileOwner && <Button onClick={() => setUserNameDialog(true)}>Change username</Button>}
                    </ListItem>
                    <ListItem>
                        <ListItemText primary={user?.biography} secondary="biography" />
                        {isProfileOwner && <Button onClick={() => setBiographyDialog(true)}>Change biography</Button>}
                    </ListItem>
                    <ListItem>
                        {isProfileOwner && <Button onClick={() => setPasswordDialog(true)}>Change password</Button>}
                    </ListItem>
                    <ListItem>
                        {isModerator && !(user?.user_type === "administrator" || user?.user_type === "moderator") && <Button onClick={handleBanUser} variant="contained" color="error">{user?.is_blocked ? "Unban" : "Ban"} user</Button>}
                    </ListItem>
                </List>
            </Box>

            <Dialog open={userNameDialogOpen}
                onClose={handleUsernameDialogOnClose} >
                <DialogTitle>Change username</DialogTitle>
                <DialogContent>
                    <TextField
                        margin="dense"
                        label="Description"
                        fullWidth
                        onChange={(e) => setNewUsername(e.target.value)}
                    />
                </DialogContent>
                <DialogActions>
                    <Button onClick={handleUsernameDialogUpdateName}>Update</Button>
                    <Button onClick={handleUsernameDialogOnClose}>Cancel</Button>
                </DialogActions>
            </Dialog>
            <Dialog open={biographyDialogOpen}
                onClose={handleBiographyDialogOnClose} >
                <DialogTitle>Change biography</DialogTitle>
                <DialogContent>
                    <TextField
                        margin="dense"
                        label="Description"
                        fullWidth
                        onChange={(e) => setNewBiography(e.target.value)}
                    />
                </DialogContent>
                <DialogActions>
                    <Button onClick={handleBiogrhaphyDialogUpdateName}>Update</Button>
                    <Button onClick={handleBiographyDialogOnClose}>Cancel</Button>
                </DialogActions>
            </Dialog>
            <Dialog open={passwordDialogOpen}
                onClose={handlePasswordDialogOnClose} >
                <DialogTitle>Change password</DialogTitle>
                <DialogContent>
                    <TextField
                        margin="dense"
                        label="Description"
                        fullWidth
                        onChange={(e) => setNewPassword(e.target.value)}
                    />
                </DialogContent>
                <DialogActions>
                    <Button onClick={handlePasswordDialogUpdate}>Update</Button>
                    <Button onClick={handlePasswordDialogOnClose}>Cancel</Button>
                </DialogActions>
            </Dialog>
        </div>
    );
}
