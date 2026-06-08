--[[
  Quest 103: Farouk's Reply
  Level Required: 19
  NPC Start: 7, NPC Complete: 15
  Prerequisites: [98]
  Rewards: EXP=3441, Gold=825
]]

local fsm = require('fsm_engine')

local quest_103 = fsm:new({
    id = 103,
    name = "Farouk's Reply",
    level_required = 19,
    npc_start = 7,
    npc_complete = 15,
    prerequisites = {98},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 103: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 103: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 103: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 103: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 103: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 103: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 103: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 103: Rewards given")
                self:give_rewards({
                    exp = 3441,
                    gold = 825,
                    items = {
                        { item_id = 45, count = 1 },
                        { item_id = 45, count = 1 },
                        { item_id = 21000009, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_103