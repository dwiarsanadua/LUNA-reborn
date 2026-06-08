--[[
  Quest 69: Class Master
  Level Required: 4
  NPC Start: 7, NPC Complete: 10
  Prerequisites: [68]
  Rewards: EXP=50
]]

local fsm = require('fsm_engine')

local quest_69 = fsm:new({
    id = 69,
    name = "Class Master",
    level_required = 4,
    npc_start = 7,
    npc_complete = 10,
    prerequisites = {68},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 69: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 69: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 69: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 69: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 69: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 69: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 69: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 69: Rewards given")
                self:give_rewards({
                    exp = 50,
                    items = {
                        { item_id = 21000006, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_69