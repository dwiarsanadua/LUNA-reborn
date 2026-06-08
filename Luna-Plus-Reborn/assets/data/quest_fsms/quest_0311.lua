--[[
  Quest 311: [C Class] Orc Shaman Elimination
  Level Required: 42
  NPC Start: 80, NPC Complete: 80
  Prerequisites: [309]
  Rewards: EXP=150976, Gold=4300
]]

local fsm = require('fsm_engine')

local quest_311 = fsm:new({
    id = 311,
    name = "[C Class] Orc Shaman Elimination",
    level_required = 42,
    npc_start = 80,
    npc_complete = 80,
    prerequisites = {309},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 311: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 311: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 311: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 44,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 311: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 311: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 311: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 311: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 311: Rewards given")
                self:give_rewards({
                    exp = 150976,
                    gold = 4300,
                    items = {
                        { item_id = 216, count = 50 },
                        { item_id = 30000499, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_311