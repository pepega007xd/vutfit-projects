import { encode_cookie } from "@/auth";
import db from "@/db";
import { Users } from "@/types";
import { Box, Button, Stack, TextField, Typography } from "@mui/material";
import { redirect } from "next/navigation";
import { cookies } from "next/headers";

export default function LoginForm() {
  return (
    <Stack
      alignItems="center" // Centers children horizontally
      justifyContent="center" // Centers children vertically
      height='100vh'
      width="100vw"
      component="form" action={async (form_data) => {
        "use server";
        const username = form_data.get("username") as string;
        const password = form_data.get("password") as string;
        const user = await db.query<Users>(
          "SELECT * FROM users WHERE username = $1 AND password_hash = crypt($2, password_hash)",
          [username, password]).first();

        if (user && !user.is_blocked) {
          (await cookies()).set(await encode_cookie({ user_id: user.user_id }));
          redirect("/");
        }
      }}>
      <Box sx={{
        display: "flex",
        flexDirection: "column",
        alignItems: "center",
        justifyContent: "center",
        gap: 4,
        backgroundColor: '#2B2B2B',
        padding: 2,
        borderRadius: 1,
        boxShadow: 3,
        width: "20em",
        height: "25em",
      }}>
        <Typography variant="h4">
          Login
        </Typography>
        <TextField id="username" label="Username" name="username" />
        <TextField id="pwd" label="Password" type="password" name="password" />
        <Button type="submit">Login</Button>
      </Box>
    </Stack >
  );
}
