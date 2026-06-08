--[[
  Quest 708: Can’t Hold Alone
  Level Required: 64
  NPC Start: 46, NPC Complete: 46
  Rewards: EXP=718287
]]

local fsm = require('fsm_engine')

local quest_708 = fsm:new({
    id = 708,
    name = "Can’t Hold Alone",
    level_required = 64,
    npc_start = 46,
    npc_complete = 46,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 708: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 708: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 708: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 16,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 708: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 63,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 708: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 28,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 708: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 708: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 708: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 708: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 708: Rewards given")
                self:give_rewards({
                    exp = 718287,
                })
            end,
            transitions = {},
        },
    },
})

return quest_708