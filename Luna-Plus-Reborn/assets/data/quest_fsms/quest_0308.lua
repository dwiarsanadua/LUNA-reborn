--[[
  Quest 308: [C Class] Lamia Elimination
  Level Required: 40
  NPC Start: 79, NPC Complete: 79
  Prerequisites: [307]
  Rewards: EXP=125391, Gold=3800
]]

local fsm = require('fsm_engine')

local quest_308 = fsm:new({
    id = 308,
    name = "[C Class] Lamia Elimination",
    level_required = 40,
    npc_start = 79,
    npc_complete = 79,
    prerequisites = {307},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 308: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 308: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 308: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 308: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 308: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 308: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 308: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 308: Rewards given")
                self:give_rewards({
                    exp = 125391,
                    gold = 3800,
                    items = {
                        { item_id = 213, count = 1 },
                        { item_id = 213, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_308