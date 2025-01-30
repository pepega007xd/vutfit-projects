// File: app/image/[image_id]/route.ts
// Author: Tomáš Brablec (xbrabl04)
// Date: 2024-12-13

import { get_image } from '@/actions';
import { NextResponse } from 'next/server';

// raw route handler to return an image with the given
// image_id from the backend
export async function GET(_: Request,
    { params }: { params: Promise<{ image_id: string }> }
) {
    const image_id = Number((await params).image_id);
    return new NextResponse(await get_image(image_id));
}
