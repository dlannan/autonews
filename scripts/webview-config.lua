local vid_thumb = { w=455, h=256 }
local num_fetch = 10

local agents = {
	"Mozilla/5.0 (Linux; Android 13; Pixel 6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/112.0.0.0 Mobile Safari/537.36",
	"Mozilla/5.0 (Linux; Android 13; Pixel 7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/112.0.0.0 Mobile Safari/537.36",
	"Mozilla/5.0 (Linux; Android 12; moto g stylus 5G) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/112.0.0.0 Mobile Safari/537.36v",
	"Mozilla/5.0 (Linux; Android 12; Redmi Note 9 Pro) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/112.0.0.0 Mobile Safari/537.36",
	"Mozilla/5.0 (Linux; Android 10; MAR-LX1A) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/112.0.0.0 Mobile Safari/537.36",
	"Mozilla/5.0 (Linux; Android 12; M2102J20SG) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/112.0.0.0 Mobile Safari/537.36",
	"Mozilla/5.0 (Linux; Android 12; DE2118) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/112.0.0.0 Mobile Safari/537.36",
	"Mozilla/5.0 (iPhone13,2; U; CPU iPhone OS 14_0 like Mac OS X) AppleWebKit/602.1.50 (KHTML, like Gecko) Version/10.0 Mobile/15E148 Safari/602.1",
	"Mozilla/5.0 (iPhone12,1; U; CPU iPhone OS 13_0 like Mac OS X) AppleWebKit/602.1.50 (KHTML, like Gecko) Version/10.0 Mobile/15E148 Safari/602.1",
	"Mozilla/5.0 (iPhone; CPU iPhone OS 12_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) FxiOS/13.2b11866 Mobile/16A366 Safari/605.1.15",
	"Mozilla/5.0 (iPhone; CPU iPhone OS 12_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.0 Mobile/15E148 Safari/604.1",
	"Mozilla/5.0 (iPhone9,3; U; CPU iPhone OS 10_0_1 like Mac OS X) AppleWebKit/602.1.50 (KHTML, like Gecko) Version/10.0 Mobile/14A403 Safari/602.1",
	"Mozilla/5.0 (Windows Phone 10.0; Android 4.2.1; Microsoft; RM-1127_16056) AppleWebKit/537.36(KHTML, like Gecko) Chrome/42.0.2311.135 Mobile Safari/537.36 Edge/12.10536",
	"Mozilla/5.0 (Linux; Android 12; SM-X906C Build/QP1A.190711.020; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/80.0.3987.119 Mobile Safari/537.36",
	"Mozilla/5.0 (Linux; Android 7.0; Pixel C Build/NRD90M; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/52.0.2743.98 Safari/537.36",
	"Mozilla/5.0 (Linux; Android 6.0.1; SGP771 Build/32.2.A.0.253; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/52.0.2743.98 Safari/537.36",
	"Mozilla/5.0 (Linux; Android 5.0.2; LG-V410/V41020c Build/LRX22G) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/34.0.1847.118 Safari/537.36",
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.135 Safari/537.36 Edge/12.246",
	"Mozilla/5.0 (X11; CrOS x86_64 8172.45.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.64 Safari/537.36",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_2) AppleWebKit/601.3.9 (KHTML, like Gecko) Version/9.0.2 Safari/601.3.9",
	"Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.111 Safari/537.36",
	"Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:15.0) Gecko/20100101 Firefox/15.0.1",
}

local cookie = [[
VISITOR_INFO1_LIVE=56vzBnuVFPc; PREF=tz=Australia.Adelaide&f6=40000000; HSID=AacP_BEoJvECM3gVl; SSID=AEC6gWS9dhKEX2eXb; APISID=h2HeNgH_NUof7D1D/A-Sxjk5aaGBxBSOpb; SAPISID=22geGjB5nB9KPiXJ/AGBbV6XP0FQ2JGVQM; __Secure-1PAPISID=22geGjB5nB9KPiXJ/AGBbV6XP0FQ2JGVQM; __Secure-3PAPISID=22geGjB5nB9KPiXJ/AGBbV6XP0FQ2JGVQM; LOGIN_INFO=AFmmF2swRQIgNSHic5LZrIZuIBfjR6axJteorFsBcUM0CFpKG457GxACIQDQwHJwulqsR05ZwEkKWJmp-N0rBg6oOJ0TVgVYsY4s7A:QUQ3MjNmeDRtUTY2WHNQaUxNZVRaRDJKNFRrZTBydk9PUS1WRFpoWXNheHpFOWQ0dHMxUjZRdDkwd2M3VjdrQjRzVGJNa1cxQmkxTVhYZWFLbUNVWFhpX01GMm43bkdyZXh3aTE3TkZhQzBQOUVwb3pxNVFqZ2xuTUJ4VjVXSTJMSmxkZHFLMWZydnZFNkU3N3kyTkl4amF1SGVISjQ0S2JB; YSC=5N6HI81eSEw; VISITOR_PRIVACY_METADATA=CgJBVRICGgA%3D; NID=511=TwMNh54c9p5S0iJB7nwYKMIDTZCdmAbiGyopoTEh8bg0dzze0yB9nXnAfcRselkp4etUeyqphy33IJG5aKg51cA15pN-uPsfFvbf4NAEXF3HRu2-mr1XqPzJLZsudYQO91PBumyfXD3lRC06xlcMuDcyxT1079PwSQUaCPM19-_4QgBGxY8oTa6wSzgQTdm9tlNSnDgLI9mE; OTZ=7191363_18_14_133470_18_395550; SID=agjNVUMr-IG492DKzmZQFCY5wtEfcBWNDNNlkEfQu0MawqqJ6bTmmZr3hQZODtI1SSh5yA.; __Secure-1PSID=agjNVUMr-IG492DKzmZQFCY5wtEfcBWNDNNlkEfQu0MawqqJnFQIAbxoMRZ4AC77rbBR3w.; __Secure-3PSID=agjNVUMr-IG492DKzmZQFCY5wtEfcBWNDNNlkEfQu0MawqqJs8YZ26q5lH0LTW6pQ3zf-Q.; wide=1; __Secure-1PSIDTS=sidts-CjIB3e41hef64_ToVP_y-ozf_-MYGyEhQ5E5Kp0dIcZkSbj7R2e3QRL2wIQiKECQBMUerBAA; __Secure-3PSIDTS=sidts-CjIB3e41hef64_ToVP_y-ozf_-MYGyEhQ5E5Kp0dIcZkSbj7R2e3QRL2wIQiKECQBMUerBAA; SIDCC=APoG2W966KXYRhA0kJnAe6PoHB7W3vWshe38wK6JXsDIYwT3QeSzyzdg8GGQrjZKqJBF4Ndt2g; __Secure-1PSIDCC=APoG2W8eylP8SiuxDjPMrTjigwYlx3N6q5s5LVhSk8uo33MTZT1lT_LW3QpqxrTIDEJjXPKEZVk; __Secure-3PSIDCC=APoG2W8_t0EdxOlMIuZMLkFF6lDVwfL_1i86-qFYw9QAIaw3nfqZc8_4NRO8S2ucjWqmL2lU9QU
]]


local headers = {
	["User-Agent"] = "Mozilla/5.0 (MSIE; Windows 10)",
	["Accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application",
	-- ["Accept-Encoding"] = "gzip, deflate, br",
	-- ["Accept-Languages"] = "en-US,en;q=0.9",
	-- ["Cache-Controls"] = "max-age=0",
	-- ["Upgrade-Insecure-Requests"] = 1,
	["Cookie"] = cookie,
	-- ["X-YouTube-Client-Name"] = "3",
	-- ["X-YouTube-Client-Version"] = "16.20",
}   
-- Add this as 6th param to write to file
local options = {
	["path"] = "res.html"
} 

return {
	vid_thumb = vid_thumb,
	num_fetch = num_fetch,
	agents = agents,
	cookie = cookie,
	headers = headers,
	options = options,
}
