"use client"

import React, { useState, useEffect } from "react";
import { get_groups } from "@/actions";
import Groups_list from "./groups_list"
import New_Group from "./new_group";
import { User_Group_Member } from "@/types";
import { get_current_user } from "@/actions";
import { RequestMembershipInGroup } from "./request_membership_in_group";
import { Box, ButtonGroup, Stack } from "@mui/material";

export default function GroupsPage() {
  const [groups, setGroups] = useState<User_Group_Member[]>([]); // Uložení seznamu skupin
  const [loading, setLoading] = useState<boolean>(true); // Stav pro načítání dat
  const [error, setError] = useState<string | null>(null); // Stav pro chybu při načítání
  const [logged, setLogIn] = useState<boolean>(false);
  // Načítání skupin
  useEffect(() => {
    const fetchGroups = async () => {
      try {
        const groupsData = await get_groups();
        setGroups(groupsData || []); // Nastavení seznamu skupin
      } catch (err) {
        setError('Failed to load groups' + err);
      } finally {
        setLoading(false);
      }
    };
    const checkLoggedIn = async () => {
      try {
        const user = await get_current_user();
        if (user) setLogIn(true);
      } catch (err) {
        setError('Failed to load current user' + err);
      }
    };
    checkLoggedIn();
    fetchGroups();
  }, []); // Tento useEffect se spustí jen jednou při prvním renderování


  const addGroup = async () => {
    const groupsData = await get_groups();
    setGroups(groupsData || []); // Nastavení seznamu skupin
  };

  const handleDeleteGroup = (groupId: number) => {
    setGroups((prevGroups) => prevGroups.filter(group => group.group_id !== groupId));
  }

  const handleOnLeaveGroup = async () => {
    const data = await get_groups();
    setGroups(data || []);
  }

  if (loading) {
    return <p>Loading groups...</p>;
  }

  if (error) {
    return <p>{error}</p>;
  }

  if (logged) {
    return (

      <Stack margin="2em" spacing="2em">
        <Box sx={{
          backgroundColor: '#2B2B2B',
          padding: 2,
          borderRadius: 1,
          boxShadow: 3,
        }}>
          <ButtonGroup variant="contained" aria-label="Basic button group">
            <New_Group onGroupAdded={addGroup} />
            <RequestMembershipInGroup />
          </ButtonGroup>
        </Box>
        <Groups_list groups={groups} onGroupDelete={handleDeleteGroup} onLeave={handleOnLeaveGroup} />
      </Stack>
    );
  } else {
    return <div>
      <p>You are not logged in</p>
    </div>
  }

}
