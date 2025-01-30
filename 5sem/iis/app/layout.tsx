import type { Metadata, Viewport } from "next";
import { Roboto } from "next/font/google";
import { StyledEngineProvider, ThemeProvider } from '@mui/material/styles';
import { Box, CssBaseline } from "@mui/material";
import "./globals.css";
import { darkTheme } from "@/theme";
import Sidebar from "@/navigation";

export const metadata: Metadata = {
  title: "Fitstagram",
  description: "Fitstagram",
};

export const viewport: Viewport = {
  initialScale: 1,
  width: 'device-width'
}

const roboto = Roboto({
  weight: ['300', '400', '500', '700'],
  subsets: ['latin'],
  display: 'swap',
  variable: '--font-roboto',
});

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {

  return (
    <html lang="en">
      <body>
        <Box sx={{ display: 'flex' }} className={`${roboto.variable} antialiased`} id="app_root">
          <StyledEngineProvider injectFirst>
            <ThemeProvider theme={darkTheme}>
              <CssBaseline />
              <Sidebar />
              {children}
            </ThemeProvider>
          </StyledEngineProvider>
        </Box>
      </body>
    </html >
  );
}
