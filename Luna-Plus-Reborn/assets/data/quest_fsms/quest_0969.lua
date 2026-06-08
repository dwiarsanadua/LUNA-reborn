--[[
  Quest 969: [Dungeon Quest] Secret Tower 2 (Advanced)
  Level Required: 50
  NPC Start: 545, NPC Complete: 545
  Rewards: EXP=190259, Gold=12128
]]

local fsm = require('fsm_engine')

local quest_969 = fsm:new({
    id = 969,
    name = "[Dungeon Quest] Secret Tower 2 (Advanced)",
    level_required = 50,
    npc_start = 545,
    npc_complete = 545,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 969: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 969: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 969: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 493,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 969: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 494,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 969: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 969: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 969: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 969: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 969: Rewards given")
                self:give_rewards({
                    exp = 190259,
                    gold = 12128,
                })
            end,
            transitions = {},
        },
    },
})

return quest_969