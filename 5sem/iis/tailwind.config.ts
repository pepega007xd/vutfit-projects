import type { Config } from "tailwindcss";

const config: Config = {
  content: [
    "./pages/**/*.{js,ts,jsx,tsx,mdx}",
    "./components/**/*.{js,ts,jsx,tsx,mdx}",
    "./app/**/*.{js,ts,jsx,tsx,mdx}",
  ],

  // https://mui.com/material-ui/integrations/interoperability/#tailwind-css
  corePlugins: {
    preflight: false,
  },
  important: '#app_root',
};
export default config;
