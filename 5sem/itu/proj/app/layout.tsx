// File: app/layout.tsx
// Author: Tomáš Brablec (xbrabl04)
// Date: 2024-12-13

import type { Metadata, Viewport } from "next";
import { Roboto } from "next/font/google";
import { StyledEngineProvider, ThemeProvider } from '@mui/material/styles';
import { Box, CssBaseline } from "@mui/material";
import "./globals.css";
import { darkTheme } from "@/theme";
import Sidebar from "@/navigation";

export const metadata: Metadata = {
  title: "Artiscope",
  description: "Image sharing platform",
};

export const viewport: Viewport = {
  initialScale: 1,
  width: 'device-width'
}

// fonts used by the application
const roboto = Roboto({
  weight: ['300', '400', '500', '700'],
  subsets: ['latin'],
  display: 'swap',
  variable: '--font-roboto',
});

// Defines the layout of each page in the application.
// `children` is replaced by rendered content of 
// respective `page.tsx` for each route.
export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {

  return (
    <html lang="en">
      <body>
        <Box
          display="flex"
          className={`${roboto.variable} antialiased`}
          id="app_root"
        >
          {/* these components add dark mode style to MaterialUI components */}
          <StyledEngineProvider injectFirst>
            <ThemeProvider theme={darkTheme}>
              {/* resets CSS to state consistent across browsers */}
              <CssBaseline />

              {/* navigation bar on the left side of the page */}
              <Sidebar />

              {/* content of the page */}
              {children}
            </ThemeProvider>
          </StyledEngineProvider>
        </Box>
      </body>
    </html >
  );
}
