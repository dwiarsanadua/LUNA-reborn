--[[
  Quest 307: [C Class] Lamia Elimination
  Level Required: 40
  NPC Start: 79, NPC Complete: 79
  Prerequisites: [305]
  Rewards: EXP=125391, Gold=3800
]]

local fsm = require('fsm_engine')

local quest_307 = fsm:new({
    id = 307,
    name = "[C Class] Lamia Elimination",
    level_required = 40,
    npc_start = 79,
    npc_complete = 79,
    prerequisites = {305},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 307: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 307: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 307: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 307: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 307: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 307: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 307: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 307: Rewards given")
                self:give_rewards({
                    exp = 125391,
                    gold = 3800,
                    items = {
                        { item_id = 212, count = 1 },
                        { item_id = 212, count = 50 },
                        { item_id = 30000499, count = 1 },
                        { item_id = 21000096, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_307