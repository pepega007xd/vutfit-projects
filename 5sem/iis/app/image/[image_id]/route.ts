import { NextResponse } from 'next/server';
import db from "@/db";

export async function GET(_: Request,
    { params }: { params: Promise<{ image_id: string }> }
) {
    const image_id = Number((await params).image_id);
    const { image_content } = await db.query<{ image_content: Uint8Array }>(
        "SELECT * FROM image_data WHERE image_id = $1", [image_id]
    ).one();
    return new NextResponse(image_content);
}
