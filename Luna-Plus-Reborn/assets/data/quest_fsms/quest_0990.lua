--[[
  Quest 990: Quest 990
  Level Required: 0
  NPC Start: 302, NPC Complete: 303
  Prerequisites: [989]
]]

local fsm = require('fsm_engine')

local quest_990 = fsm:new({
    id = 990,
    name = "Quest 990",
    level_required = 0,
    npc_start = 302,
    npc_complete = 303,
    prerequisites = {989},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 990: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 302,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 990: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 990: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 302,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 990: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 303,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 990: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 990: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 303,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 990: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 990: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21000611, count = 5 },
                        { item_id = 21000511, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_990