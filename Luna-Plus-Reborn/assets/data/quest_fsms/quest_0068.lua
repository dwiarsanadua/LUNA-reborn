--[[
  Quest 68: Collecting Information
  Level Required: 4
  NPC Start: 7, NPC Complete: 9
  Rewards: EXP=50
]]

local fsm = require('fsm_engine')

local quest_68 = fsm:new({
    id = 68,
    name = "Collecting Information",
    level_required = 4,
    npc_start = 7,
    npc_complete = 9,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 68: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 68: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 68: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 68: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 68: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 68: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 68: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 68: Rewards given")
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

return quest_68