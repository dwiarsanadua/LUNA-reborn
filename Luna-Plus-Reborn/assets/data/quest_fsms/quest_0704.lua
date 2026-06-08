--[[
  Quest 704: [Party]Confusion
  Level Required: 0
  NPC Start: 12, NPC Complete: 12
  Prerequisites: [702]
  Rewards: EXP=179621
]]

local fsm = require('fsm_engine')

local quest_704 = fsm:new({
    id = 704,
    name = "[Party]Confusion",
    level_required = 0,
    npc_start = 12,
    npc_complete = 12,
    prerequisites = {702},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 704: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 12,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 704: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 704: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 126,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 704: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 149,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 704: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 150,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 704: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 12,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 704: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 704: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 12,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 704: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 704: Rewards given")
                self:give_rewards({
                    exp = 179621,
                })
            end,
            transitions = {},
        },
    },
})

return quest_704