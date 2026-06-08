--[[
  Quest 159: Forgetful Dwarf
  Level Required: 30
  NPC Start: 5, NPC Complete: 28
  Rewards: EXP=41148, Gold=7500
]]

local fsm = require('fsm_engine')

local quest_159 = fsm:new({
    id = 159,
    name = "Forgetful Dwarf",
    level_required = 30,
    npc_start = 5,
    npc_complete = 28,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 159: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 159: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 159: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 54,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 159: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 26,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 159: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 159: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 159: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 159: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 159: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 159: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 159: Rewards given")
                self:give_rewards({
                    exp = 41148,
                    gold = 7500,
                    items = {
                        { item_id = 74, count = 1 },
                        { item_id = 75, count = 1 },
                        { item_id = 21000095, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_159