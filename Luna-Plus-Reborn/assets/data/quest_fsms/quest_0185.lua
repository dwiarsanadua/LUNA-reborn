--[[
  Quest 185: The Orc Leader
  Level Required: 42
  NPC Start: 10, NPC Complete: 10
  Prerequisites: [184]
  Rewards: EXP=108703, Gold=11800
]]

local fsm = require('fsm_engine')

local quest_185 = fsm:new({
    id = 185,
    name = "The Orc Leader",
    level_required = 42,
    npc_start = 10,
    npc_complete = 10,
    prerequisites = {184},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 185: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 185: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 185: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 59,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 185: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 185: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 185: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 185: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 185: Rewards given")
                self:give_rewards({
                    exp = 108703,
                    gold = 11800,
                })
            end,
            transitions = {},
        },
    },
})

return quest_185