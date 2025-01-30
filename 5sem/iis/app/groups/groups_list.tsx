"use client"

import { List, ListItem } from "@mui/material";
import Group from "./group";
import { User_Group_Member } from "@/types";


type GroupsListProps = {
    groups: User_Group_Member[];
    onGroupDelete: (groupId: number) => void;
    onLeave: () => void;
};

// function for showing one group and do actions with it
export default function Groups_list({ groups, onGroupDelete, onLeave }: GroupsListProps) {
    if (groups.length > 0) {
        return (
            <div>
                <List>
                    {groups.map((group) => (
                        <ListItem key={group.group_id}>
                            <Group groupNumber={group.group_id} onDelete={onGroupDelete} onLeave={onLeave}></Group>
                        </ListItem>
                    ))}
                </List>
            </div>
        );
    }
    else {
        return <p>No groups to show</p>
    }
}
