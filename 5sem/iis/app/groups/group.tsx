"use client"

import React, { useEffect, useState } from "react";
import { delete_group, get_group_info, get_user_in_group, has_moderating_rights, move_ownership_of_group, remove_user_from_group } from "@/actions";
import { Groups, Users } from "@/types";

import { Button, List, ListItem, ListItemText, ButtonGroup, ListSubheader, Divider, Typography, Box } from "@mui/material";
import UpdateGroup from "./update_group";
import { get_current_user } from "@/actions";
import AddUserIntoGroup from "./add_user_into_group";
import HandleRequestForGroup from "./handle_requests_from_users";

type GroupProps = {
  groupNumber: number;
  onDelete: (groupId: number) => void;
  onLeave: () => void;
};

export default function Group({ groupNumber, onDelete, onLeave }: GroupProps) {
  const [group, setGroup] = useState<Groups | undefined>(undefined); // Stav pro uložení dat skupiny
  const [loading, setLoading] = useState<boolean>(true); // Stav pro načítání
  const [error, setError] = useState<string | null>(null); // Stav pro chyby
  const [isOwner, setIsOwner] = useState<boolean>(false);
  const [membersOfGroup, setMembers] = useState<Users[]>([]);

  useEffect(() => {
    const fetchGroupData = async () => {
      try {
        const data = await get_group_info(groupNumber);
        setGroup(data);
      } catch (error) {
        setError("Failed to load group data." + error);
      } finally {
        setLoading(false);
      }
    };
    fetchGroupData();
  }, [groupNumber]); // Závislost na groupNumber, aby se opětovně načetla při změně

  useEffect(() => {
    const fetchIsOwner = async () => {
      try {
        const user = await get_current_user();
        if (user && user.user_id == group?.owner_id) {
          setIsOwner(true);
        }
      } catch (error) {
        setError("Failed to load current user." + error);

      }
    };
    fetchIsOwner();
  }, [group]);

  useEffect(() => {
    const fetchUsers = async () => {
      try {
        const users = await get_user_in_group(groupNumber);
        setMembers(users || []);
      } catch (error) {
        setError("Failed to load users as members of group." + error);

      }
    };
    fetchUsers();
  }, [groupNumber]);


  const handleDeleteGroup = async () => {
    try {
      await delete_group(groupNumber);
      alert("Group was deleted");
      onDelete(groupNumber);
    } catch (error) {
      alert("Group failed to be deleted" + error);

    }
  };


  const handleGroupUpdate = async () => {
    try {
      const data = await get_group_info(groupNumber);
      setGroup(data);
    } catch (err) {
      setError("Failed to load group data." + err);

    } finally {
      setLoading(false);
    }
  };

  const handleRemoveUserFromGroup = async (userId: number) => {
    try {
      const data = await remove_user_from_group(userId, groupNumber);
      if (data) {
        const users = await get_user_in_group(groupNumber);
        if (users) setMembers(users);
      }
    } catch (error) {
      setError("Failed to delete user from group." + error);

    }
  };

  const handleTransferOwnershipOfGroup = async (userId: number) => {
    try {
      const data = await move_ownership_of_group(userId, groupNumber);
      if (data) {
        setGroup(data);
        setIsOwner(false);
      }

    } catch (error) {
      setError("Failed to transfer ownership of group." + error);

    }
  };

  const handleAddUser = async () => {
    try {
      const data = await get_user_in_group(groupNumber);
      if (data) setMembers(data);
    } catch (error) {
      setError("Failed to load user from group." + error);

    }
  };

  const [isModerator, setIsModerator] = useState(false);
  useEffect(() => {
    const fetchModerator = async () => {
      const data = await has_moderating_rights();
      setIsModerator(data);
    }
    fetchModerator();
  }, []);

  const [currentUser, setCurrentUser] = useState<Users | undefined>(undefined);
  useEffect(() => {
    const fetchCurrentUser = async () => {
      const user = await get_current_user();
      setCurrentUser(user);
    }
    fetchCurrentUser();
  }, []);

  const handleOnLeaveGroup = async (userId: number) => {
    try {
      const data = await remove_user_from_group(userId, groupNumber);
      if (data) {
        onLeave();
        const users = await get_user_in_group(groupNumber);
        if (users) setMembers(users);
        alert("You left the group.");
      }
    } catch (error) {

      alert("Failed to left the group" + error)

    }
  };


  if (loading) {
    return <p>Loading group...</p>;
  }

  if (error) {
    return <p>{error}</p>;
  }

  return (

    <Box sx={{
      backgroundColor: '#2B2B2B',
      padding: 2,
      borderRadius: 1,
      boxShadow: 3,
      width: "50vw",
    }}>
      <div>
        <div>
          <Typography variant="h5">{group?.group_name}</Typography>
          <p>{group?.description}</p>
        </div>
        <div>
          <List subheader={<ListSubheader component="div" id="nested-list-subheader" sx={{
            backgroundColor: '#2B2B2B', color: "white"
          }}><Typography variant="h5" >Group members</Typography></ListSubheader>}>
            {membersOfGroup.map((member) => (
              <div key={member.user_id}>
                <ListItem key={member.user_id}>
                  {group?.owner_id === member.user_id && <ListItemText primary={"Owner: " + member.username} />}
                  {group?.owner_id !== member.user_id && <ListItemText primary={"Member: " + member.username} />}
                  {isOwner && group?.owner_id !== member.user_id && (
                    <div>
                      <Button onClick={() => handleTransferOwnershipOfGroup(member.user_id)}>Transfer Ownership</Button>
                      <Button onClick={() => handleRemoveUserFromGroup(member.user_id)}>Remove User from Group</Button>
                    </div>
                  )}
                  {!isOwner && currentUser?.user_id === member.user_id && <Button onClick={() => { handleOnLeaveGroup(member.user_id) }}>Leave Group</Button>}

                </ListItem>
                <Divider />
              </div>
            ))}
          </List>

          <Box sx={{
            display: "flex",
            flexDirection: "row",
            gap: 1
          }}>
            {!isOwner && isModerator && <Button variant="outlined" color="error" onClick={handleDeleteGroup}>Delete Group</Button>}
            {isOwner && (<div>
              <ButtonGroup variant="contained" aria-label="Basic button group">
                <Box sx={{
                  display: "flex",
                  flexDirection: "row",
                  gap: 1
                }}>
                  <Button variant="outlined" color="secondary" onClick={handleDeleteGroup}>Delete Group</Button>
                  <UpdateGroup group={group} onGroupUpdate={handleGroupUpdate} />
                  <AddUserIntoGroup groupId={groupNumber} onAddUserIntoGroup={handleAddUser} />
                  <HandleRequestForGroup groupId={groupNumber} onAddUserIntoGroup={handleAddUser} />
                </Box>
              </ButtonGroup>
            </div>)}
            <Button variant="outlined" href={`/group/${groupNumber}`}>Group posts</Button>
          </Box>
        </div>
      </div>
    </Box>
  );
}
