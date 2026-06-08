--[[
  Quest 967: [Dungeon Quest] Familiar Towers (Advanced)
  Level Required: 50
  NPC Start: 545, NPC Complete: 545
  Rewards: EXP=240995, Gold=12128
]]

local fsm = require('fsm_engine')

local quest_967 = fsm:new({
    id = 967,
    name = "[Dungeon Quest] Familiar Towers (Advanced)",
    level_required = 50,
    npc_start = 545,
    npc_complete = 545,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 967: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 967: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 967: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 480,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 967: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 482,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 967: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 484,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 967: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 485,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 967: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 967: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 967: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 967: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 967: Rewards given")
                self:give_rewards({
                    exp = 240995,
                    gold = 12128,
                })
            end,
            transitions = {},
        },
    },
})

return quest_967