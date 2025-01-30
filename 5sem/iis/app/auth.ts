
import { cookies } from "next/headers";
import { JWTPayload, jwtVerify, SignJWT } from "jose";
import { NextResponse } from "next/server";
import { ResponseCookie } from "next/dist/compiled/@edge-runtime/cookies";
import { UserType } from "@/types"
import { get_current_user } from "./actions";

const secret_key = process.env.NODE_ENV === "production"
  ? process.env.JWT_SECRET_KEY
  : "development key";

const secret = new TextEncoder().encode(secret_key);

const expiration_seconds = 60 * 60; // 1 hour

interface CookieData extends JWTPayload {
  user_id: number
}

export async function encode_cookie(data: CookieData): Promise<ResponseCookie> {
  const cookie = await new SignJWT(data)
    .setProtectedHeader({ alg: 'HS256' })
    .setIssuedAt()
    .setExpirationTime(`${expiration_seconds}sec`)
    .sign(secret);
  return {
    name: "session",
    httpOnly: true,
    value: cookie,
    sameSite: "strict",
    maxAge: expiration_seconds
  }
}

export async function get_session(): Promise<CookieData | undefined> {
  const cookie = (await cookies()).get("session")?.value;
  if (!cookie) return;
  const { payload } = await jwtVerify(cookie, secret, { algorithms: ["HS256"] })
  return payload as CookieData;
}



export async function update_session() {
  const cookie = await get_session();
  if (!cookie) return;

  const res = NextResponse.next();
  res.cookies.set(await encode_cookie(cookie));
  return res;
}

export function has_access(user: UserType, role: UserType): boolean {
  const map = (user_type: UserType) => {
    switch (user_type) {
      case "administrator": return 3;
      case "moderator": return 2;
      case "user": return 1;
      default: return 0;
    }
  }
  return map(user) >= map(role);
}

export async function current_user_has_access(role: UserType): Promise<boolean> {
  const user = await get_current_user();
  const map = (user_type: UserType) => {
    switch (user_type) {
      case "administrator": return 3;
      case "moderator": return 2;
      case "user": return 1;
      default: return 0;
    }
  }
  return map(user?.user_type ?? "guest") >= map(role);
}
