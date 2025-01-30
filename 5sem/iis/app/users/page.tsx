import db from "@/db";
import { UserType, type Users } from "@/types";
import { Delete } from "@mui/icons-material";
import { Box, Button, IconButton, MenuItem, Stack, Table, TableBody, TableCell, TableContainer, TableHead, TableRow, TextField, Typography } from "@mui/material";
import { revalidatePath } from "next/cache";
import { UserTypeSelector } from "./user_type";
import { UserLink } from "@/post/post";
import { current_user_has_access } from "@/auth";

export default async function Users() {
  const users = [...(await db.query<Users>("SELECT * FROM Users"))];

  return (
    <Stack width="100%" margin="2em" direction="row" spacing="2em">
      <Box component="form"
        action={async (form_data) => {
          "use server";
          if (! await current_user_has_access("administrator")) { return; }
          await db.query(
            `INSERT INTO Users (username, is_blocked, password_hash, user_type) 
             VALUES ($1, false, crypt($2, gen_salt('bf')), $3)`,
            [
              form_data.get("username") as string,
              form_data.get("password") as string,
              form_data.get("user_type") as UserType,
            ]);
          revalidatePath("/users");
        }}
        sx={{
          display: 'flex',
          flexDirection: 'column',
          gap: 2,
          width: "100%"
        }}
      >
        <Typography variant="h4">Add user</Typography>
        <TextField
          label="Name"
          variant="outlined"
          name="username"
          required
        />
        <TextField
          label="Password"
          variant="outlined"
          name="password"
          type="password"
        />
        <TextField
          select
          name="user_type"
          label="User type"
          defaultValue="user"
          slotProps={{
            select: {
              defaultValue: "user"
            }
          }}
        >
          <MenuItem value="user">User</MenuItem>
          <MenuItem value="moderator">Moderator</MenuItem>
          <MenuItem value="administrator">Administrator</MenuItem>
        </TextField>
        <Button
          variant="contained"
          type="submit"
        >
          Add user
        </Button>
      </Box >
      <TableContainer component={Box}>
        <Table>
          <TableHead>
            <TableRow>
              <TableCell></TableCell>
              <TableCell>Username</TableCell>
              <TableCell>User type</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {users.map((user) => (
              <TableRow
                key={user.user_id}
              >
                <TableCell component="th" scope="row">
                  <IconButton onClick={async () => {
                    "use server";
                    if (! await current_user_has_access("administrator")) { return; }
                    await db.query(
                      "DELETE FROM users WHERE user_id = $1",
                      [user.user_id]);
                    revalidatePath("/users");
                  }}>
                    <Delete />
                  </IconButton>
                </TableCell>
                <TableCell component="th" scope="row">
                  <UserLink user={user} />
                </TableCell>
                <TableCell component="th" scope="row">
                  <UserTypeSelector user={user} />
                </TableCell>
              </TableRow>
            ))}
          </TableBody>
        </Table>
      </TableContainer>
    </Stack >
  );
}
