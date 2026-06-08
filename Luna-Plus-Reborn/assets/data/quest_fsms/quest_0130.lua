--[[
  Quest 130: Request for Help
  Level Required: 3
  NPC Start: 2, NPC Complete: 50
  Rewards: EXP=26
]]

local fsm = require('fsm_engine')

local quest_130 = fsm:new({
    id = 130,
    name = "Request for Help",
    level_required = 3,
    npc_start = 2,
    npc_complete = 50,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 130: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 130: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 130: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 130: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 130: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 130: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 50,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 130: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 130: Rewards given")
                self:give_rewards({
                    exp = 26,
                    items = {
                        { item_id = 21000006, count = 8 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_130