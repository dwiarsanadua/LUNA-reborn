--[[
  Quest 612: The Adventure Begins
  Level Required: 1
  NPC Start: 4, NPC Complete: 200
]]

local fsm = require('fsm_engine')

local quest_612 = fsm:new({
    id = 612,
    name = "The Adventure Begins",
    level_required = 1,
    npc_start = 4,
    npc_complete = 200,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 612: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 612: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 612: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 612: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 200,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 612: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 612: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 612: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 612: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 200,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 612: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 612: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_612