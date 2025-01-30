"use client";

import { change_user_type } from "@/actions";
import { Users, UserType } from "@/types";
import { MenuItem, TextField } from "@mui/material";
import { useState } from "react";

export function UserTypeSelector({ user }: { user: Users }) {
  const [user_type, set_user_type] = useState(user.user_type);

  return (
    <TextField
      select
      sx={{
        width: "15em"
      }}
      value={user_type}
      label="User Type"
      onChange={async (e) => {
        const new_type = e.target.value as UserType
        set_user_type(new_type);
        await change_user_type(user.user_id, new_type);
      }}
    >
      <MenuItem value={"user"}>User</MenuItem>
      <MenuItem value={"moderator"}>Moderator</MenuItem>
      <MenuItem value={"administrator"}>Administrator</MenuItem>
    </TextField>
  );
}
