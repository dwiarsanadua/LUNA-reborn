--[[
  Quest 2082: Favor of the High Elves
  Level Required: 75
  NPC Start: 55, NPC Complete: 70
  Prerequisites: [279]
]]

local fsm = require('fsm_engine')

local quest_2082 = fsm:new({
    id = 2082,
    name = "Favor of the High Elves",
    level_required = 75,
    npc_start = 55,
    npc_complete = 70,
    prerequisites = {279},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2082: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 70,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2082: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2082: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 28,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2082: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 109,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2082: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 70,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2082: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2082: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2082: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 70,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2082: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2082: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 185, count = 25 },
                        { item_id = 186, count = 25 },
                        { item_id = 187, count = 1 },
                        { item_id = 188, count = 1 },
                        { item_id = 30000483, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2082