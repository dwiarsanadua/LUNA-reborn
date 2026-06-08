--[[
  Quest 361: [Special] Batteries Not Included
  Level Required: 150
  NPC Start: 22, NPC Complete: 22
  Prerequisites: [358]
]]

local fsm = require('fsm_engine')

local quest_361 = fsm:new({
    id = 361,
    name = "[Special] Batteries Not Included",
    level_required = 150,
    npc_start = 22,
    npc_complete = 22,
    prerequisites = {358},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 361: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 22,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 361: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 361: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 52,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 361: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 55,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 361: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 361: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 22,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 361: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 361: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 22,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 361: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 361: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 279, count = 1 },
                        { item_id = 281, count = 1 },
                        { item_id = 283, count = 1 },
                        { item_id = 30000466, count = 1 },
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_361